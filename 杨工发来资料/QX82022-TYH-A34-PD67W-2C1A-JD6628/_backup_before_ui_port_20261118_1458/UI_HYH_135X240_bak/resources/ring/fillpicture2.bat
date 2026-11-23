@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: ImageMagick magick 批量合成
:: 只遍历当前目录png，不递归子文件夹
:: 叠加坐标：
:: 12,12    label\sleep.png
:: 160,12   label\empty.png
:: 173,17   label\l.png
:: 184,17   label\0.png
:: 192,17   label\x.png
:: 198,17   label\2.png

for %%f in (*.png) do (
    :: 跳过label文件夹里的图片，防止重复处理
        echo 正在处理：%%f
        magick "%%f" a/w.png -geometry +46+75 -composite "%%~nf.png"
)
echo.
echo 全部处理完成，输出文件后缀为 _out.png
pause
