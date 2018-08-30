
# LZW decompression implementation

12 fixed width implementation of lzw decompression, sources used to develop this were from Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange.

### Compile

``` >> gcc -o demoDecompLzw decompress.c lzw.c ```

### Usage

**demoDecompLzw      input_file     output_file**

``` >> ./demoDecompLzw compressedfile1.z output.txt ```

### Further Work
- Implement custom memory pools, to reduce malloc calls.
- Use a output buffer for the file writes.
- After reading "Clean Code" by Robert Martin, a variety of things should be implemented to improve the code such as:
  - Replace magic numbers with constants or variable names.
  - Refactor the files into smaller, more module, Single responsibility modules.
