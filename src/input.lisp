(begin
     (defun fact (n)
        (if (== n 1)
            1
            (* n (fact (- n 1)))))

    (lst := (quote ("Adam" "Beata" "Celina" "Dorota" "Edek")))
    (is-woman := (str.has_suffix "a"))
    (print (lst |> seq.filter is-woman))
    (print (lst |> seq.at 2))
    (print (lst |> seq.at 5))
)
