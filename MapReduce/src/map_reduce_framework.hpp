/*
 * This is a map reduce framework.
 * User can register a container(hashmap), and start a mapper in which user can write key-value pair to the container.
 * User can provide a key groupper and start a reducer, we will group the keys and pass the related values to reducer.
 * The result is another container(hashmap) with grouped keys and their values.
 */
