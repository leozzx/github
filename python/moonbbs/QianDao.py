# -*- coding: utf-8 -*-

from selenium import webdriver
import random
import time

import util

def qian_dao (driver, donenum):
  print "number of qiandao to be execute: " + str(15 - donenum)
  # goto each page and click QianDao
  pageid = ['46', '41', '52', '58', '69', '48', '65', '45', '57', '68', '47', '56', '54', '76', '44']
  for i in pageid[donenum:]:
    url = "http://www.moonbbs.com/forum-"+i+"-1.html"
    util.get_url(driver, url)
    driver.find_element_by_id("qiandao").click();
    time.sleep(1)



def get_date (driver):
  url = "http://www.moonbbs.com/home.php?mod=spacecp&ac=profile&op=info"
  util.get_url(driver, url)
  currenttime = driver.find_elements_by_class_name("mtn");
  datetime = currenttime[len(currenttime)-1].text[7:]

  #remove h m s
  date = datetime[0: datetime.find(" ")]

  #modify date, 2015-9-10 -> 2015-09-10
  datelist = date.split('-', 3)
  date = datelist[0] + '-'
  if (len(datelist[1]) == 1):
    date += '0'
  date += datelist[1] + '-'
  if (len(datelist[2]) == 1):
    date += '0'
  date += datelist[2]

  return date



def get_qiandao_num (driver, date):
  url = "http://www.moonbbs.com/home.php?mod=spacecp&ac=credit&op=log"
  util.get_url(driver, url)
  stri = u"每日签到积分"
  elements = driver.find_elements_by_link_text(stri.encode("utf-8"));
  qiandao = 0
  for e in elements:
    ancestor_tr = e.find_element_by_xpath(".//ancestor::tr")
    try :
      ee = ancestor_tr.find_elements_by_tag_name("td")
      if (ee[3].text.startswith(date)):
        qiandao += 1
    except:
      pass
  return qiandao

