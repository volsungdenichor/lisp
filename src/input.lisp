(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (let foo (pipe (partial + 1) (partial * 10)))

    (let lst (list 2 3 4 9 ))
    (print lst)
    (print (seq.map foo lst))
)
