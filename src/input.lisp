(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))
    (print (fact 4))
    (print (fact 5))
    (print (fact 6))

    (let val 123)
    (let func (lambda (arg) (/ arg 10.0)))
    (let lst (quote ("Adam" "Beata" "Celina" "Dorota" "Edek")))
    (let is-woman (str.has_suffix "a"))
    (print lst)
    (print (seq.filter is-woman lst))

)
