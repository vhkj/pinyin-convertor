## Options

```
-1 	Combine two dictionary. Output PinYin of combined chinese words. Must set -l and -r option.
-2 	Convert specified Chinese dictionary to PinYin dictionary. Must set -i option.
-C 	Capital the first character of all words.
-F 	Only output the first character of all words.
-w 	Only Convert input Chinese text to PinYin like and then quit.
-l 	Set the left side dictionary
-r 	Set the right side dictionary
-i 	Set input file to convert
-o 	Set output file. Default is out.dict
--help Show this message
```

## Examples

### 组合两个汉语字典, 输入组合全拼字典, 大小为两个字典行数的乘积

`PinYinConvertor -1 -l familyName.txt -r secondName.txt -o out.dict`

### 转换指定汉语字典为拼音字典

`PinYinConvertor -2 -i inputFile.dict -o outputFile.dict`

### 只转换命令行输入的汉字

`PinYinConvertor -w 世界你不好`

### 以上都可以带上选项 `-F`, 表示只输出汉字全拼首字母. 带上选项 `-C` 会将每个汉字单词全拼首字母大写
