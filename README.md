# wave-function-kollapse
Custom implementation of the wave function collapse algorithm

## GENERAL:
[uwu](https://www.youtube.com/watch?v=zIRTOgfsjl0&ab_channel=DVGen)
[jdh te amo](https://www.youtube.com/watch?v=TO0Tx3w5abQ&ab_channel=jdh)
- sacar obfuscacion de OOP / hacerlo una funcion de C simple
- añadir direcciones
- Flags:
```
ONFAILURE
{ 
	FULL_RESTART ---> Restart whole function
	NUKE -----------> Return specified zone to initial values
	BACKTRACK ------> Mejor nombre q NUKE
	CUSTOM ---------> Use custom user provided algorithm
	CONTINUE -------> Ignore
}
```
```
ENTROPY
{
	LOWEST_ENTROPY -> Find lowest entropy tile
	RANDOM ---------> Choose tile at random
	INDEX ----------> By index 0 to N
}
```
```
...
```

## OVERLAP:
- area de analisis N*N
- chequea si algun estado original coincide con los analizados durante la propagacion

## RULES:
- reglas definidas por el usuario
- añadirle probabilidad (WEIGHT)

## Ideas
### Cluster collapse
Proceso:
- [1] Asignar valor default a toda la matriz
- [2] Seleccionar sector n*n (cluster)
- [3] Colapsar cluster
- [4] Si hay error, volver cluster a su estado original y volver a [3]
- [5] Repetir [2] hasta que la matriz este totalmente colapsada
Puede ser con overlap o sin. -> Flag
