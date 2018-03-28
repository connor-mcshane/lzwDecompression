
# LZW decompression implementation

12 fixed width implementation of lzw decompression, sources used to develop this were from Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange.

### Compile

``` >> gcc -o demoDecompLzw decompress.c lzw.c ```

### Usage

**demoDecompLzw      input_file     output_file**

``` >> ./demoDecompLzw compressedfile1.z output.txt ```

### Further Work
- Implementation of LZW compressor
- Implement custom memory management for dictionary and phrases

