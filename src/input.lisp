(begin
    (let val 123)
    (let func (lambda (arg) (/ arg 10.0)))
    (let lst (quote ("Adam" "Beata" "Celina" "Dorota" "Edek")))
    (let is-woman (str.has_suffix "a"))
    (print lst)
    (print (seq.filter is-woman lst))

)
