# -*- coding: utf-8 -*-

from selenium import webdriver
from sets import Set
import random
import time

import QianDao
import Post
import util

######### global sets ##################
uidSet = Set()
tidSet = Set()
######### global sets ##################

############ function to get thread_id and uid ###############
def getIds(driver, post):
  #get the thread id from post url
  first = post.index('-') + 1
  second = post[first:].index('-')
  thread_id = post[first:first + second]

  if thread_id in tidSet:
    raise RuntimeError("pick another one")
  else:
    tidSet.add(thread_id)

  util.get_url(driver, post)
  #get the user id by finding the href that contains "op=send"
  l = driver.find_elements_by_class_name("xi2")
  uidList = []
  for s in l:
    idx = s.get_attribute("href").find("op=send")
    if idx != -1:
      uidList.append(s.get_attribute("href"))

  if len(uidList) < 2:
    raise RuntimeError("pick another post")

  rand = random.randrange(0, len(uidList))
  first = uidList[rand].find('uid')
  uid = uidList[rand][first+4:]

  if uid in uidSet:
    raise RuntimeError("pick another one")
  else:
    uidSet.add(uid)

  ids = [thread_id, uid]
  return ids
#######################################################

def login(driver, user, pwd):
  url = "http://www.moonbbs.com/member.php?mod=logging&action=login"
  util.get_url(driver, url)
  driver.find_element_by_name("username").send_keys(user)
  driver.find_element_by_name("password").send_keys(pwd)
  driver.find_element_by_name("loginsubmit").click();
  return
############# end login #############

def logout(driver):
  url = "http://www.moonbbs.com/forum-52-1.html"
  util.get_url(driver, url)
  driver.find_element_by_class_name("goout").click();
  time.sleep(3)
  return
############# end logout #############

def fenxiang_dazhaohu_liuyan(driver, date):
  stri = u"发起分享"
  fenxiang = Post.get_numbers(driver, stri.encode("utf-8"), date);
  stri = u"打招呼"
  dazhao = Post.get_numbers(driver, stri.encode("utf-8"), date);
  stri = u"留言"
  liuyan = Post.get_numbers(driver, stri.encode("utf-8"), date);

  print "before: fenxiang(" + str(fenxiang) + ")dazhaohu(" + str(dazhao) + ")liuyan(" + str(liuyan) + ")"
  if ((fenxiang >= 3) and (dazhao >= 5) and (liuyan >= 5)):
    return

  #goto XianLiao and find posts
  PostList = Post.prepare_post_list(driver)
  for post in PostList:
    try:
      ids = getIds(driver, post);
    except:
      continue
    if (fenxiang < 3):
      try:
        Post.fen_xiang(driver, ids[0])
        fenxiang += 1
      except:
        pass
    if (dazhao < 5):
      try:
        Post.da_zhao_hu(driver, ids[1])
        dazhao += 1
      except:
        pass
    if (liuyan < 5):
      try:
        Post.liu_yan(driver, ids[1])
        liuyan += 1
      except:
        pass
  print "after: fenxiang(" + str(fenxiang) + ")dazhaohu(" + str(dazhao) + ")liuyan(" + str(liuyan) + ")"
  return
############# end FenXiang,DaZhaoHu,LiuYan #############

############# main function ###############
if __name__ == "__main__":
  # get webdriver
  #driver = webdriver.Chrome(util.resource_path("./chromedriver"))
  chrome_options = webdriver.ChromeOptions()
  chrome_options.add_argument("--incognito")

  driver = webdriver.Chrome("./chromedriver", chrome_options=chrome_options)
  #driver = webdriver.Chrome(util.resource_path("./chromedriver"), chrome_options=chrome_options)

  #amanda991@sina.com
  usernames = [ "zzhuang_6@sina.com", "amanda.991@live.cn", "791614645@qq.com", "litongs@nmsu.edu", "zzx.zhuang@gmail.com", "414081812@qq.com"]
  passwords = [ "zzx693236", "991220", "991220", "991220", "123991220", "zzx693236"]

  for index in range(len(usernames)):
    login(driver, usernames[index], passwords[index])
    #qiandao
    date = QianDao.get_date(driver)
    qiandao = QianDao.get_qiandao_num(driver, date)
    QianDao.qian_dao(driver, qiandao)
    #fenxiang, dazhaohu, liuyan
    fenxiang_dazhaohu_liuyan(driver, date)

    logout(driver)

  #quit
  driver.quit()


