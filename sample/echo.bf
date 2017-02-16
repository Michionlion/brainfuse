setup vars = c0 is 0 = c1 is prompt char (62)
++++ ++++[>++++ ++++<-]     get 64 in c1
>--<                        subtract 2

=MAIN=
+[                         always enter loop (c0 is always 1)
>.                         print prompt
>>                         move to c3 for char list = leave c2 as 0 for marker/spacer
loops and gets input while input != 10
,---- ---- --[
>,---- ---- --
]
<                           go back to last char
[++++ ++++ ++<]             add 10 to that char if it is not 0 then go back and repeat
>                           move to first char
[.>]                        print it and move to next char = exit loop if next char is zero

now reset
<                           go back to last char (or c2 if first char was 10)
[<]                         go back to first zero (c2)
++++ ++++ ++.---- ---- --   print newline and reset
.<.<.                          reset to c0
]
