
function onCall()
   local global = var.new( "Global Stuff", nil )

   print( "Test Start.\r\n" )
   global["This"] = 1
   global["is"] = "A speed test"
   global["of"] = "the variable system code."
   global["Im worried"] = 10303
   global["That, its very"] = "slow"
   global["And"] = "if I cant figure out how to make it faster... we have a problem"
   global["hopefully"] = "using some inline method calls instead will speed it up"
   global["ifnot"] = "fuck my life!!!"
   print( "Test finish.\r\n" )

end
