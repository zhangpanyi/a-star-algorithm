# AStar
C++ 实现A*算法，使用二叉堆和小对象分配器进行优化

# 测试环境
vs2013
<a href="http://s796.photobucket.com/user/zhangpanyi/media/Link/circlegallery_zps4321f105.png.html" target="_blank"><img src="http://i796.photobucket.com/albums/yy247/zhangpanyi/Link/circlegallery_zps4321f105.png" border="0" alt=" photo circlegallery_zps4321f105.png"/></a>

测试数据
===================================
<table>

<tr>
<td>地图大小</td>
<td>允许斜走</td>
<td>耗时</td>
</tr>

<tr>
<td>100*100</td>
<td>否</td>
<td>0.001s</td>
</tr>

<tr>
<td>100*100</td>
<td>是</td>
<td>0.000s</td>
</tr>

<tr>
<td>1000*1000</td>
<td>否</td>
<td>0.137s</td>
</tr>

<tr>
<td>1000*1000</td>
<td>是</td>
<td>0.006s</td>
</tr>

</table>
