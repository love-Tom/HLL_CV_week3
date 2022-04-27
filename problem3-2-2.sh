#!/bin/bash
# @file		problem3-2-2.sh
# @author	zhaoqicheng(zhaoqicheng2022@163.com/623146682)
# @brief	猜数字游戏
# @details	⾸先让系统随机⽣成⼀个数字，给这个数字定⼀个范围（1-60），让⽤户输⼊猜的数字，对输⼊
#		进⾏判断，如果不符合要求，就给予⾼或低的提示，猜对后则给出猜对⽤的次数。
# @date		2022-4-15
# @copyright	All create by zhaoqicheng.

#方法：使用 RANDOM 随机整数函数并取余
value=$((RANDOM%60+1))	#学长要猜的数字

#使用while循环进行游戏

while :
do
	echo "已生成一个[1-60]的整数，快来猜@wzh"
	read -p "请输入你猜的数字： " number
	if [ ${number} -eq ${value} ];then
		echo "猜对了，恭喜寿命-1"
		exit
	elif [ ${number} -lt ${value} ];then
		echo "猜小了，往大了猜！"
	else
		echo "猜大了，往小了猜！"
	fi
done









