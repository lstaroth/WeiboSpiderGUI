使用C调用Python，混合编程笔记  
#目的  
1.由于微博网页结构再次更新导致之前的微博爬虫版本已经不能使用，所以对Python爬虫进行了一次更新  
2.学习下如何使用Python与C进行交互，即如何将Python嵌入C++程序中  
3.记录下进行此项工程时遇到的几个坑，以及如何结果/规避  

前段时间实习工作的时候有听过Electron架构，本来是想学习一波的，但是由于对nodejs了解实在太少，想进行下去难度还是挺大的，不过如果是Python和C结合的话应该没啥问题，于是就开始这个小项目啦

#TIPS:
1.Python文件的main函数会在导入时执行，最好别有main函数/稳定执行的代码，避免不必要的时间浪费
2.学习Python/C API最好的地方当然是他们的官方文档啦，选择合适的Python版本，查看API介绍即可，附上网址：https://docs.python.org/3.6/c-api/intro.html#objects-types-and-reference-counts

