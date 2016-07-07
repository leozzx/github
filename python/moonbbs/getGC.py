# -*- coding: utf-8 -*-

from selenium import webdriver
from sets import Set
import random
import time
import sys

import util


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

def QiangGC (driver):
  url = "http://www.moonbbs.com/thread-161629-1-1.html"
  util.get_url(driver, url)
  element = driver.find_elements_by_xpath("//div/dl/li/em");
  cnt = int(element[4].text[2])
  if (cnt <= 0):
    print "No GC avaliable ", cnt
    return
  element = driver.find_element_by_name("ijoin")
  element.click()
  time.sleep(1)
  element = driver.find_element_by_tag_name("button")
  print element.text
  element.click()
  print "get GC, please check if success"

############# main function ###############
if __name__ == "__main__":
  if len(sys.argv) != 4:
    print "Usage: python getGC.py email password interval"
    sys.exit(0)

  # get webdriver
  #driver = webdriver.Chrome(util.resource_path("./chromedriver"))
  chrome_options = webdriver.ChromeOptions()
  chrome_options.add_argument("--incognito")

  driver = webdriver.Chrome("./chromedriver", chrome_options=chrome_options)
  #driver = webdriver.Chrome(util.resource_path("./chromedriver"), chrome_options=chrome_options)

  username = sys.argv[1]
  password = sys.argv[2]
  interval = sys.argv[3]

  login(driver, username, password)

  while True:
    QiangGC(driver)
    time.sleep(int(interval)*60)

  logout(driver)

  #quit
  driver.quit()


