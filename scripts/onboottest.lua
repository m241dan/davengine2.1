function onCall()
   print( "Starting test...\n" )
   test = var.get( "Triggers" )
   test["onTestCall"] = nil
end;
