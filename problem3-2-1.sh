#!/bin/bash
# @file         problem3-2-1.sh
# @author       zhaoqicheng(zhaoqicheng2022@163.com/623146682)
# @brief        shell实现算术运算
# @details      提示WZH学长输入两个数字，使其相减并输出结果，若学长未按要求输入>则提示
# @date         2022-4-25
# @copyright    All create by zhaoqicheng.


read -p "请输入第一个数字的值： " number1
expr ${number1} + 1 >/dev/null 2>&1
[ $? -ne 0 ] && echo " ${number1} is not number" || echo "${number1} is a  number"
read -p "请输入第二个数字的值:  " number2
[ $? -ne 0 ] && echo " ${number2} is not number" || echo "${number2} is a  number"
let sum=${number1}+${number2}
echo "${number1}+${number2}=sum"


