#coding:GB18030


import requests
import re
import json
from lxml import etree


# 获取指定网页的Cookies
def GetCookies():
    # 获取dict_data
    UrlSection = requests.session()
    url = "https://passport.weibo.com/visitor/genvisitor"
    UrlSection.headers.update(
        {
            "User-Agent": "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0"
        }
    )

    data = {
        "cb": "gen_callback",
        "fp": '{"os":"1","browser":"Chrome59,0,3071,115","fonts":"undefined","screenInfo":"1680*1050*24","plugins":"Enables Widevine licenses for playback of HTML audio/video content. (version: 1.4.8.984)::widevinecdmadapter.dll::Widevine Content Decryption Module|Shockwave Flash 26.0 r0::pepflashplayer.dll::Shockwave Flash|::mhjfbmdgcfjbbpaeojofohoefgiehjai::Chrome PDF Viewer|::internal-nacl-plugin::Native Client|Portable Document Format::internal-pdf-viewer::Chrome PDF Viewer"}'
    }
    response = UrlSection.post(url, data=data)
    pattren=re.compile(r"\((.*)\)")
    data=pattren.findall(response.text)[0]
    dict_data=json.loads(data)["data"]
    tid=dict_data["tid"]
    confidence=dict_data["confidence"]
    where=dict_data["new_tid"]
    if where:
        where=3
    else:
        where=2
    while(len(str(confidence))<3):
        confidence="0"+str(confidence)
    tid="KCEsfUfkAmyXExt9tiPN61f32Vvh4wViWQaeHptBZLc="
    url="https://passport.weibo.com/visitor/visitor?a=incarnate"\
        "&t="+str(tid)+ \
        "&w=" + str(where) + \
        "&c="+str(confidence)+\
        "&gc="\
        "&cb=cross_domain" \
        "&from=weibo"
    response=UrlSection.get(url)
    data=pattren.findall(response.text)[0]
    dict_data=json.loads(data)["data"]
    sub=dict_data["sub"]    #没有
    subp=dict_data["subp"]
    url="https://login.sina.com.cn/visitor/visitor?a=crossdomain&cb=return_back"\
        +"&s="+str(sub)\
        +"&sp="+str(subp)\
        +"&from=weibo"
    response=UrlSection.get(url)
    return UrlSection.cookies,UrlSection.headers

# base62解码
def base62(string):
    alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    base = len(alphabet)
    strlen = len(string)
    num = 0
    idx = 0
    for char in string:
        power = (strlen - (idx + 1))
        num += alphabet.index(char) * (base ** power)
        idx += 1
    return num

# 去除页面信息的特殊比较方式
def HtmlCompare(pasthtml,html,page):
    if len(pasthtml) != len(html):
        return False
    pastpageinfo = "&root_comment_ext_param=&page=" + str(page) + "&filter=hot&"
    currentpageinfo = "&root_comment_ext_param=&page=" + str(page+1) + "&filter=hot&"
    pasthtml = pasthtml.replace(pastpageinfo,currentpageinfo)
    if pasthtml == html:
        return True
    else:
        return False

# 获取异步加载所需ID
def GetWeiboId(Url):
    pattern=re.compile(r"\d\/(.*)\?type")
    content=pattern.search(Url).group(1)
    string1=str(base62(content[0]))
    string2=str(base62(content[1:5]))
    while(len(string2)<7):
        string2="0"+string2
    string3=str(base62(content[5:]))
    while (len(string3) < 7):
        string3 = "0" + string3
    id=string1+string2+string3
    return id

# 获取微博正文信息
def CatchWeiboContent(Url):
    Cookies, Headers = GetCookies()
    MySection=requests.session()
    MySection.cookies=Cookies
    MySection.headers=Headers
    Response = MySection.get(Url)
    Page = etree.HTML(Response.text)
    Content = Page.xpath('//title/text()')[0]
    Content = Content.encode("GB18030")
    Content = str(Content).replace("\n", " ")
    return Content

# 爬取评论
def CatchComments(Url):
    Cookies, Headers = GetCookies()
    MySection=requests.session()
    MySection.cookies=Cookies
    MySection.headers=Headers
    id = GetWeiboId(Url)

    page = 1
    pasthtml = ""
    comments = list()

    while(True):
        CommentsUrl="https://weibo.com/aj/v6/comment/big?ajwvr=6"+"&id="+str(id)+"&page="+str(page)+"&filter=hot"+"&from=singleWeiBo"
        Response=MySection.get(CommentsUrl)
        html=json.loads(Response.text)["data"]["html"]
        if HtmlCompare(pasthtml,html,page) == True :
            break
        text=etree.HTML(html)
        pagecomments = text.xpath('//div[@class="list_li S_line1 clearfix"]/*/div[@class="WB_text"]')
        pagelikepoints = text.xpath('//div[@class="list_li S_line1 clearfix"]//*/span[@node-type="like_status"]/child::*[2]//text()')
        for i in range(len(pagecomments)):
            comment = pagecomments[i].xpath("text()")
            comment = ",".join(comment[1:])[1:].strip()
            comments.append(comment.encode("GB18030"))
            if pagelikepoints[i] == u"赞":
                comments.append("0".encode("GB18030"))
            else:
                comments.append(pagelikepoints[i].encode("GB18030"))
        page+=1
        pasthtml = html

    # 返回值形如 ["好好看","235","nice","230","good","23","好看","5"....] 一个评论接一个字符型数值形式
    return comments

# 爬取图片
def CatchPictures(Url):
    Cookies, Headers = GetCookies()
    MySection=requests.session()
    MySection.cookies=Cookies
    MySection.headers=Headers
    id = GetWeiboId(Url)

    page = 1
    pasthtml = ""
    pictures = list()

    while(True):
        CommentsUrl="https://weibo.com/aj/v6/comment/big?ajwvr=6"+"&id="+str(id)+"&page="+str(page)+"&filter=hot"+"&from=singleWeiBo"
        Response=MySection.get(CommentsUrl)
        html=json.loads(Response.text)["data"]["html"]
        if HtmlCompare(pasthtml,html,page) == True :
            break
        text=etree.HTML(html)
        ids = text.xpath('//li[@action-type="comment_media_img"]/attribute::action-data')
        for id in ids:
            print type(id)
            id = id.split("&")
            id = id[0][4:]
            picturesUrl = "https://wx3.sinaimg.cn/bmiddle/" + id + ".jpg"
            pictures.append(picturesUrl.encode("GB18030"))
        page += 1
        pasthtml = html

    return  pictures

#    https://weibo.com/1778375693/EoI32tiax?type=comment
#Url="https://weibo.com/5044281310/H8mUW70yn?type=comment"
#print CatchWeiboContent(Url)
#CatchComments(Url)
#CatchPictures(Url)