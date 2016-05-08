# Interceptor
<img src="https://ci.appveyor.com/api/projects/status/github/PrakaramJoshi/Interceptor?branch=master&amp;svg=true"></img>
</br>
Interceptor is a library which helps to generate runtime call diagrams for applications. Using this library you can also see in realtime which functions are being executed at any moment. The purpose to write this was to help a developer understand the dynamics of the application and how various functions/classes interact with each other at runtime. The image below shows the output for the following code snippet when run 
</br>
<strong>Code Snippet</strong>
</br>
<img src="https://github.com/PrakaramJoshi/Interceptor/blob/master/CodeSnippet.PNG"></img>
<img src="https://github.com/PrakaramJoshi/Interceptor/blob/master/CodeSnippet2.PNG"></img>
</br>
</br>
<strong>Timeline graph</strong>
<img src="https://github.com/PrakaramJoshi/Interceptor/blob/master/TimelineGraph.PNG"></img>
</br>
<p>
Blue connection show a function on the left called function on the right.</br>
Red connection show a function on the right called function on the left.</br>
All the functions are arranged left to right in the order they were first run.</br>
</p>
#Usage:</br>
Link your application with this library (see instructions below on what changes are required in your application). Place the Interceptor.config in the directory where your applications's executable is present. Then run your application. Interceptor will generate the TImeline graph/Call Dependency Graph / or in real time show the function calls. This is for Windows Only.

#How to integrate with an application:</br>
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
