# ocallm

Training a (tiny) language model in OCaml, from scratch

## Data

The model is trained against articles taken from [Wikipedia](https://en.wikipedia.org/).
For obvious reasons, we cannot include the dataset in this GitHub repository.
However, the raw data can be easily downloaded from [this link](https://en.wikipedia.org/wiki/Wikipedia:Database_download).
Simply download and decompress any of the Wikipedia XML dumps, and put the resulting XML file in the `data/` folder.
Then, running the python script `extraction.py` will extract all Wikipedia pages from the XML
file and store them in a format that can be consumed by the rest of the pipeline.

Be aware that depending on the XML dump you choose, the total content size can range from ~1GB to over 80GB. The extraction script has been tested on `simplewiki-20211001-pages-articles-multistream.xml` (927M). This exact XML dump of Wikipedia is available as a torrent at [this address](https://wm-torrents.theresnotime.co.uk/2021/simple/simplewiki-20211001-pages-articles-multistream.xml.bz2.torrent).


## Data sanitization

Once extracted, *all* wikipedia articles are stored in a separate file, in [wikitext](https://en.wikipedia.org/wiki/Help:Wikitext) format.
Wikitext is a markup language and therefore wikitext files contain tags, code, and other symbols that should be removed to obtain pure English text ready to be fed to the language model.
This preprocessing step is performed by a custom *wikitext sanitizer* written in [OCaml](https://ocaml.org) (see `lib/sanitizer.ml`).


## Commands

+ `ocallm sanitize`: run the wikitext sanitizer and generate `.sanitized` text files in `data/articles/`