(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (let lst (quote ("Adam" "Beata" "Celina" "Dorota" "Edek")))
    (let is-woman (str.has_suffix "a"))
    (print (lst |> seq.filter is-woman))
)
