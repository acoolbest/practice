#!/usr/bin/python
#coding:utf-8

from selenium import webdriver


from selenium.webdriver.chrome.options import Options
options = Options()
options.add_argument('--headless')
options.add_argument('--no-sandbox')
# options.add_argument('--disable-dev-shm-usage')
#driver = webdriver.Chrome(executable_path="/usr/bin/chromedriver",chrome_options=options)
driver = webdriver.Chrome(chrome_options=options)
#driver = webdriver.Chrome()
driver.get('https://www.baidu.com')

print driver.title

driver.quit()
