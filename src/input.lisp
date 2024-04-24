(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (defun is_even (n)
        (== (% n 2) 0))

    (print "5! =" (fact 5))

    (let lst (list 2 3 4 9 11 14 16 20 21 ))
    (print lst)
    (print (pipe
        seq.filter is_even)
        seq.map (* 10))
        seq.rev)
        lst))
)
