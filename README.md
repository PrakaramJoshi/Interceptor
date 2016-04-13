# Interceptor
For Windows Only
Interceptor generates call graph, realtime function call visualization.</br>
<img src="https://ci.appveyor.com/api/projects/status/github/PrakaramJoshi/Interceptor?branch=master&amp;svg=true"></img>
</br>
Interceptor implements _penter and _pexit (x86 and x64) and uses the generated hooks to track function calls. Any Project can use Interceptor to see live function calls or dependency wheel for the functions by following these steps:</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1) Turn off all optimizations(linker and compiler)</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2) add /Gh /GH flags to compile options( commandline options)</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;3) set Project>Property Pages> C/C++ > Code Generation > Basic Runtime checks to Both(/RTC1,equiv. to /RTCsu)(/RTC1)</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4) In the Linker options , specify Interceptor.lib as an input</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;5) Re-compile your project.</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6) Place the Interceptor.dll in the directory where the executable is generated.</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;7) place Interceptor.config there to specify custom settings</br>
  
#Build:</br>
  Interceptor solution builds with Visual studio 2015. The solution has two projects :</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1) Interceptor : This creates the Interceptor.dll/lib</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2) InterceptorTest: This is a demo project.</br>
