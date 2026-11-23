@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: 1. 参数设置
:: ==========================================
set width=106
set height=106
set offsetx=15
set offsety=0
set "rantan=30,36,46,33"
:: ==========================================
:: 2. 批量处理图片
:: ==========================================
for %%F in (*.jpg) do (
	set /a h1=%rantan[1]%-%offsety%
	set /a h2=%rantan[3]%
	set /a h3=%height%-!h1!-!h2!
	set /a w1=%rantan[0]%-%offsetx%
	set /a w2=%rantan[2]%
	set /a w3=%width%-!w1!-!w2!
	set /a ow=!w1!+!w2!
	set /a oh=!h1!+!h2!
	magick %%F +repage -crop %width%x!h1!+0+0 +repage A%%~nF.png
	magick %%F +repage -crop !w1!x!h2!+0+!h1! +repage B%%~nF.png
	magick %%F +repage -crop !w3!x!h2!+!o1!+!h1! +repage C%%~nF.png
	magick %%F +repage -crop %width%x!h3!+0+!oh! +repage D%%~nF.png
)

	
echo All PNG files have been processed and overwritten.

pause