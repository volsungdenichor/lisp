(begin
            (let val 123)
            (let func (lambda (arg) (/ arg 10.0)))
            (print val)
            (print (func val))
        )
