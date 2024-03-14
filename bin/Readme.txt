## 组合两个汉语字典, 输入组合全拼字典, 大小为两个字典行数的乘积

PinYinConvertor -1 -l familyName.txt -r secondName.txt -o out.dict

## 转换指定汉语字典为拼音字典

PinYinConvertor -2 -i inputFile.dict -o outputFile.dict

## 只转换命令行输入的汉字

PinYinConvertor -w 世界你不好

> 以上都可以带上选项 -F, 表示只输出汉字全拼首字母. 带上选项 -C 会将每个汉字单词全拼首字母大写