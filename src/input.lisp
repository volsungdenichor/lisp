(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (defun is_even (n)
        (== (% n 2) 0))

    (let add-1-then-mult-by-10 (pipe (+ 1) (* 10)))

    (let lst (list 2 3 4 9 11 14 16 20 21 ))
    (print lst)
    (print (seq.map add-1-then-mult-by-10 lst))
    (print (lst |> seq.filter is_even |> seq.map (* 10) ))
)
