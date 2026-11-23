@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: 1. 参数设置
:: ==========================================

:: 设置填充颜色为黄色
set "color=#F80000"

:: 定义坐标数组（支持多个矩形）
:: 格式: "x1,y1,x2,y2"
:: 你可以根据需求修改或添加坐标
set "ranges[0]=44,43,92,79"
set "ranges[1]=59,87,77,105"

:: 计算坐标数组的数量
set count=0
:count_loop
if defined ranges[%count%] (
    set /a count+=1
    goto :count_loop
)

:: ==========================================
:: 2. 批量处理图片
:: ==========================================
for %%F in (*.png) do (
    set "draw_commands="

    :: 构造 draw 参数
    for /L %%i in (0,1,%count%-1) do (
        set "range=!ranges[%%i]!"
        for /f "tokens=1-4 delims=," %%a in ("!range!") do (
            set "draw_commands=!draw_commands! rectangle %%a,%%b %%c,%%d"
        )
    )

    :: 执行 ImageMagick 命令
    :: 注意: 这里直接覆盖原文件（-write mpr:img +delete -write mpr:img）
    magick "%%F" -fill "%color%" -draw "!draw_commands!" "%%~nF.png"
)

echo All PNG files have been processed and overwritten.
