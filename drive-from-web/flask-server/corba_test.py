#!/usr/bin/python3

import CORBA
orb = CORBA.ORB_init()
o = orb.string_to_object("corbaloc::host.example.com/fortune")
o = o._narrow(Fortune.CookieServer)
print(o.get_cookie())
