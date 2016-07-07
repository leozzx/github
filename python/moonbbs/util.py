from selenium import webdriver
from selenium import common

def resource_path(relative):
  if hasattr(sys, "_MEIPASS"):
    return os.path.join(sys._MEIPASS, relative)
  return os.path.join(relative)

def get_url (driver, url):
  retry = 10
  driver.set_page_load_timeout(5)
  while True:
    try:
      driver.get(url)
    except common.exceptions.TimeoutException:
      retry -= 1
      if (retry < 0):
        raise RuntimeError("retry failed")
      continue
    else:
      break
  return
