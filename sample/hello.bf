				first step is to get 72 in cell 1
++++++++		insert 8 into cell 0
[>+++++++++<-]	loop 8 times inserting 9 each time into cell 1 (counting to 72)
>				dataptr is at c0 move to c1
.				print 'H'
---.			print 'E'
+++++++.		print 'L'
.				print 'L'
+++.			print 'O'
<+++++			set c0 to 5
[>>++++++<<-]	loop 4 times inserting 6 each time into c2 (counting to 30)
>>++.<			print ' '
++++++++.		print 'W'
--------.		print 'O'
+++.			print 'R'
------.			print 'L'
--------.		print 'D'
>+.				print '!'
<<+++++			set c0 to 5
[>>>++<<<-]		loop 5 times inserting 2 each time into c3 (counting to 10)
>>>.			print '\n'
