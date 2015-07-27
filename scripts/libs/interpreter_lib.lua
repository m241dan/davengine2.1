function davInterp( object, table, input )
   local value, cmd, command

   cmd, input = oneArg( input )
   command = table[cmd]

   if( command == nil ) then
      for i, v in pairs( table ) do
         if( isPrefix( i, cmd ) == true ) then
            command = v
            break
         end
      end
   end

   if( command == nil or command[3] > object:getLevel() ) then
      object:msg( "Bad command, try again.\r\n" )
      return
   end

   value = command[1]( object, input )
   if( value ~= nil ) then
      account:msg( value )
   end

end
