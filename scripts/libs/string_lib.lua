function oneArg( input )
   if( string.find( input, " " ) == nil ) then
      return input, nil
   end
   return string.match( input, "(%a+)%s+(.+)" )
end

function isPrefix( input, prefix )
   local start, ending = string.find( input, prefix )
   if( start ~= 1 ) then
      return false
   end
   return true 
end
