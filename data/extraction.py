import xml.etree.ElementTree as ET
import os.path as FS
import re
from os import makedirs

class Extractor:
  """Extract wikipedia articles as separate text files"""

  def __init__(self, src : str, dst : str):
    assert FS.isfile(src), (f"{src} is not a valid file")
    assert FS.splitext(src)[1] == ".xml", (f"{src} is not a .xml file")
    assert FS.isdir(dst), (f"{dst} is not a valid directory")
    self.bad_tokens = re.compile(r"\W")
    self.src = src
    self.dst = dst
    self.pages = 0
    self.current_batch_size = 0
    self.current_batch = 1

  def sanitize_title(self, content: str) -> str:
    return re.sub(self.bad_tokens, "_", content)

  def sanitize_content(self, content: None | str) -> str:
    if content is None:
      return ""
    else:
      return content
    
  def create_page(self, title: str):
    self.current_batch_size += 1
    self.pages += 1
    if self.current_batch_size > 10:
      self.current_batch_size = 0
      self.current_batch += 1
    dirname = FS.join(self.dst, f'batch_{self.current_batch}/')
    if not FS.exists(dirname):
      makedirs(dirname)
    return FS.join(dirname, title + '.txt')

  def extract_page(self, title : str, content : str):
    filename = self.create_page(title)
    with open(filename, "w") as file:
      file.write(content)
      file.flush()
    print(f"extracted page {title :15}")

  def extract(self):
    title = None
    content = None
    for _, elem in ET.iterparse(self.src):
      if elem.tag == "title":
        title = self.sanitize_title(elem.text)
      if elem.tag == "text":
        content = self.sanitize_content(elem.text)
        self.extract_page(title, content)
    print(f"Done! (extracted {self.pages} pages in {self.current_batch} batches)")

if __name__ == "__main__":
  src = "simplewiki-20211001-pages-articles-multistream.xml"
  dst = "articles/"
  Extractor(src, dst).extract()