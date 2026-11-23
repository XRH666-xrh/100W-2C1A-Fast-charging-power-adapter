@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: 1. 参数设置
:: ==========================================

:: ==========================================
:: 2. 批量处理图片
:: ==========================================
for %%F in (*.jpg) do magick %%F -crop 106x36+0+0 +repage A%%~nF.png
for %%F in (*.jpg) do magick %%F -crop 30x33+0+36 +repage B%%~nF.png
for %%F in (*.jpg) do magick %%F -crop 30x33+76+36 +repage C%%~nF.png
for %%F in (*.jpg) do magick %%F -crop 106x37+0+69 +repage D%%~nF.png
	
echo All PNG files have been processed and overwritten.
