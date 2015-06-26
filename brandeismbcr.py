
import poplib
import smtplib
from email import parser
from fitchburg import *


def get_messages():
  pop_conn = poplib.POP3_SSL('pop.gmail.com')
  pop_conn.user('brandeismbcr')
  pop_conn.pass_('hyt158crashplan')
  #Get messages from server:
  messages = [pop_conn.retr(i) for i in range(1, len(pop_conn.list()[1]) + 1)]
  # Concat message pieces:
  messages = ["\n".join(str(mssg[1])) for mssg in messages]
  #Parse message intom an email object:
  messages = [parser.Parser().parsestr(mssg) for mssg in messages]
  for message in messages:
    print(message['subject'])
  pop_conn.quit()
  return messages


def get_returnaddr(messages):
  addrs = []
  for m in messages:
    s = m[0].as_string().split(None)
    ss = ''.join(s)
    apo = ss.split("'")
    colon = [f.split(':') for f in apo]
    for f in colon:
      if f[0] == 'Return-Path':
        addrs.append(f[1].split('<')[1].split('>')[0])
  return addrs


def get_report():
  g = getTrains()
  return g.report


def send_email(toaddr, fromaddr, p, msg):
  print('Connecting to gmail server...')
  server = smtplib.SMTP('smtp.gmail.com:587')
  server.starttls()
  if type(p) is list and len(p) > 0:
    p = '\n'.join(p)
  try:
    server.login(fromaddr, p)
  except:
    print('Bad password. Login name must be same as From: fromaddr')
  server.sendmail(fromaddr, toaddr, msg)
  print('Email sent.')
  server.quit()
  return


def email_control():
  home = 'brandeismbcr@gmail.com'
  p = 'hyt158crashplan'
  msg = get_messages()
  repo = get_report()
  if len(msg) > 0:
    addrs = get_returnaddr(msg)
    print('Sent to: ')
    for add in addrs:
      send_email(add, home, p, repo)
      print(add)
  print(repo)
  
  return


################
if __name__ == "__main__":
  email_control()



