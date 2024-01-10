# wave-function-kollapse
Custom implementation of the wave function collapse algorithm

## Ideas:

### GENERAL:
- sacar obfuscacion de OOP / hacerlo una funcion de C simple
- Flags:
`
ONFAILURE { 
	FULL_RESTART, -> Restart whole function
	NUKE, ---------> Return specified zone to initial values
	CUSTOM, -------> Use custom user provided algorithm
	CONTINUE ------> 
}
`

### MÉTODOS:
OVERLAP:
- area de analisis N*N
- chequea si algun estado original coincide con los analizados durante la propagacion

RULES:
- reglas definidas por el usuario
- añadirle probabilidad (WEIGHT)
