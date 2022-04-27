#!/bin/bash
# @file		problem3-2-3.sh
# @author	zhaoqicheng(zhaoqicheng@163.com/623146682)
# @brief	bash for循环打印符合要求的字符串
# @details	利⽤ bash for 循环打印下⾯这句话中字⺟数不⼤于 6 的单词。 
#		"I am oldboy teacher welcome to oldboy training class"
# date		2022-4-25
# copyright	All create by zhaoqicheng 

echo "I am oldboy teacher welcome to oldboy training class" > sentence.txt
for i in `cat sentence.txt`
do
        if [ ${#i} -lt 6 ];then
                echo $i
        fi
done




