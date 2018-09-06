# BaseTools

### 目标
 * 使用pthread+curl实现的多线程、数据下载基本功能，用于验证跨平台的可行性。
 
***

### 说明
##### BaseTools.framework说明：
* **LICurlTask**: 使用curl封装的任务，依赖在线程池中运行。
* **LIDownloadTask**: 使用curl封装的下载模型，拥有自己的下载线程，不依赖线程池。

***

##### Demo中宏开关说明：
* **ThreadsInPool**：控制线程池中线程的数量
* **UsingCurlTask**：用于选择下载模型。**UsingCurlTask==0**，使用LIDownloadTask下载数据；**UsingCurlTask==1**，使用LICurlTask下载数据。