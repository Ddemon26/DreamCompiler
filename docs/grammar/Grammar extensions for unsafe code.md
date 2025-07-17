// Source: §23.2 Unsafe contexts
unsafe_modifier
: 'unsafe'
;

unsafe_statement
: 'unsafe' block
;

// Source: §23.3 Pointer types
pointer_type
: value_type ('*')+
| 'void' ('*')+
;

// Source: §23.6.2 Pointer indirection
pointer_indirection_expression
: '*' unary_expression
;

// Source: §23.6.3 Pointer member access
pointer_member_access
: primary_expression '->' identifier type_argument_list?
;

// Source: §23.6.4 Pointer element access
pointer_element_access
: primary_expression '[' expression ']'
;

// Source: §23.6.5 The address-of operator
addressof_expression
: '&' unary_expression
;

// Source: §23.7 The fixed statement
fixed_statement
: 'fixed' '(' pointer_type fixed_pointer_declarators ')' embedded_statement
;

fixed_pointer_declarators
: fixed_pointer_declarator (','  fixed_pointer_declarator)*
;

fixed_pointer_declarator
: identifier '=' fixed_pointer_initializer
;

fixed_pointer_initializer
: '&' variable_reference
| expression
;

// Source: §23.8.2 Fixed-size buffer declarations
fixed_size_buffer_declaration
: attributes? fixed_size_buffer_modifier* 'fixed' buffer_element_type
fixed_size_buffer_declarators ';'
;

fixed_size_buffer_modifier
: 'new'
| 'public'
| 'internal'
| 'private'
| 'unsafe'
;

buffer_element_type
: type
;

fixed_size_buffer_declarators
: fixed_size_buffer_declarator (',' fixed_size_buffer_declarator)*
;

fixed_size_buffer_declarator
: identifier '[' constant_expression ']'
;