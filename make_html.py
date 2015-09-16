#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys
import runs

htmltemplate="""
<html>
<script>
function refresh(node)
{
   var times = 10000; // gap in Milli Seconds;

   (function startRefresh()
   {
      var address;
      if(node.src.indexOf('?')>-1)
       address = node.src.split('?')[0];
      else 
       address = node.src;
      node.src = address+"?time="+new Date().getTime();

      setTimeout(startRefresh,times);
   })();

}

window.onload = function()
{
  var node = document.getElementById('refresh');
  refresh(node);
  // you can refresh as many images you want just repeat above steps
}
</script>
<head>
<title>Dakikalık Toplam GM Vuruşları (son bir saat)</title>
<meta name="author" content="Halil Gamsizkan">
<meta charset="UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="keywords" content="Geiger-Muller Counter, Geiger Sayacı, Cosmic Rays, Kozmik Işınlar, Muons, Müonlar, Physics, Fizik">
</head>
<body>
<table>
<tr> <td align='right'> Alış #:</td> <td style="font-family:Courier; color:blue;"> %RUN% </td> </tr>
<tr> <td align='right'> Toplam GM Vuruş sayısı:</td> <td style="font-family:Courier; color:blue;"> %HITS% </td> </tr>
<tr> <td align='right'> Çalışma süresi (gün):</td> <td style="font-family:Courier; color:blue;"> %DAYS% </td> </tr>
</table>
 
<!-- Started: %STARTED% <br/> -->
 
<img id="refresh" src="last_hour.png">

<hr>
<!-- begin htmlcommentbox.com -->
 <div id="HCB_comment_box"><a href="http://www.htmlcommentbox.com">HTML Comment Box</a> is loading comments...</div>
 <link rel="stylesheet" type="text/css" href="//www.htmlcommentbox.com/static/skins/bootstrap/twitter-bootstrap.css?v=0" />
 <script type="text/javascript" id="hcb"> /*<!--*/ if(!window.hcb_user){hcb_user={};} (function(){var s=document.createElement("script"), l=(""+window.location).replace(/'/g,"%27") || hcb_user.PAGE, h="//www.htmlcommentbox.com";s.setAttribute("type","text/javascript");s.setAttribute("src", h+"/jread?page="+encodeURIComponent(l).replace("+","%2B")+"&mod=%241%24wq1rdBcg%24iW2.BBlZhxyjbfDkRevO61"+"&opts=17182&num=10");if (typeof s!="undefined") document.getElementsByTagName("head")[0].appendChild(s);})(); /*-->*/ </script>
<!-- end htmlcommentbox.com -->
</body>
</html>
"""


if __name__ == '__main__':
    dataDir="data"
    runnum=runs.whichRun()
    k="gm_%05d.dat" % runnum
    try: rundata=runs.readRunData(os.path.join(dataDir,k))
    except:
        print "No such run:" , runnum
        sys.exit(1)
    
    hits=rundata[0]
    start=rundata[1]
    days=rundata[3]
    
    html=htmltemplate.replace("%RUN%", "%05d" % runnum).replace("%HITS%", str(hits)).replace("%STARTED%", str(start)).replace("%DAYS%", "%6.1f" %  days)
    of = file("index.html", "w")
    of.write(html)
    of.close()
        
        
