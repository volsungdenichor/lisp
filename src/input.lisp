(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (let add-1-then-mult-by-10 (pipe (partial + 1) (partial * 10)))

    (let lst (list 2 3 4 9 ))
    (print lst)
    (print (seq.map add-1-then-mult-by-10 lst))
    (print (seq.map fact lst))
)
