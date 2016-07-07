# -*- coding: utf-8 -*-

from selenium import webdriver
import random
import time

import util


def prepare_post_list(driver):
  PostList = []

  url = "http://www.moonbbs.com/forum-52-1.html"
  util.get_url(driver, url)
  postlist = driver.find_elements_by_class_name("xst")
  index = [18, 22, 28, 29, 31, 33, 24, 34, 32, 36, 38, 40, 41, 42, 43, 44, 23]
  for i in index:
    PostList.append(postlist[i].get_attribute("href"))

  index = [15, 22, 28, 29, 31, 20, 24, 18]
  url = "http://www.moonbbs.com/forum-58-1.html"
  util.get_url(driver, url)
  postlist = driver.find_elements_by_class_name("xst")
  for i in index:
    PostList.append(postlist[i].get_attribute("href"))

  url = "http://www.moonbbs.com/forum-65-1.html"
  util.get_url(driver, url)
  postlist = driver.find_elements_by_class_name("xst")
  for i in index:
    PostList.append(postlist[i].get_attribute("href"))

  random.shuffle(PostList)
  return PostList[:20]




def get_numbers(driver, name, date):
  numbers = 0

  url = "http://www.moonbbs.com/home.php?mod=spacecp&ac=credit&op=log&suboperation=creditrulelog"
  util.get_url(driver, url)
  element = driver.find_element_by_link_text(name)
  ancestor_tr = element.find_element_by_xpath(".//ancestor::tr")
  try :
    ee = ancestor_tr.find_elements_by_tag_name("td")
    if (ee[8].text.startswith(date)):
      numbers = ee[2].text
  except:
    pass
  return numbers


def fen_xiang(driver, thread_id):
  # FenXiang
  url = "http://www.moonbbs.com/home.php?mod=spacecp&ac=share&type=thread&id="+thread_id;
  util.get_url(driver, url)
  driver.find_element_by_id("sharesubmit_btn").click()
  time.sleep(1)

def da_zhao_hu(driver, uid):
  # DaZhaoHu
  url = "http://www.moonbbs.com/home.php?mod=spacecp&ac=poke&op=send&uid="+uid;
  util.get_url(driver, url)
  driver.find_element_by_id("pokesubmit_btn").click()
  time.sleep(1)

def liu_yan(driver, uid):
  # LiuYan
  url = "http://www.moonbbs.com/space-uid-"+uid+".html"
  util.get_url(driver, url)
  s = random.randrange(1,6)
  driver.find_element_by_id("comment_message").send_keys("[em:"+str(s)+":]")
  driver.find_element_by_id("commentsubmit_btn").click()
  time.sleep(1)

