from sys import argv


def dijkstra_score(G, shortest_distances, v, w):
    return shortest_distances[v] + G[v][w]


def dijkstra(G, source):
    unprocessed = set(G.keys())  # vertices whose shortest paths from source have not yet been calculated
    unprocessed.remove(source)
    shortest_distances = {source: 0}

    for i in xrange(len(G) - 1):
        # find a vertex with the next shortest path, i.e. minimal Dijkstra score
        m, closest_head = float('inf'), 0
        for tail in shortest_distances:
            for head in G[tail]:
                if head in unprocessed:
                    d = dijkstra_score(G, shortest_distances, tail, head)
                    if d < m:
                        m, closest_head = d, head

        unprocessed.remove(closest_head)
        shortest_distances[closest_head] = m

        # in case G is not fully connected
    for vertex in unprocessed:
        shortest_distances[vertex] = float('inf')

    return shortest_distances


def get_graph():
    filename = argv[1]
    graph = {}
    with open(filename) as g:
        for line in g:
            l = line.split()
            vertex = int(l.pop(0))
            graph[vertex] = {}
            for x in l:
                adj_vert, distance = map(int, x.split(","))
                graph[vertex][adj_vert] = distance
    print "Got graph. Ex: line 1:", graph
    return graph


def main():
    G = get_graph()
    """ Input is adjacency list on vertices labelled 1 to n, including segment length. 

    Example line of file: 
    1   3,45    92,4 

    This means that v. 1 is adjacent to v. 3 with edge length 45 and adjacent to v. 92 with edge length 4. 
    """
    source = int(raw_input("Enter source vertex: "))
    destination_vertices = map(int, raw_input("List destination vertices:\n").split())

    distances = dijkstra(G, source)

    print "From vertex %d:" % source
    for vertex in destination_vertices:
        print "The distance to vertex %d is %d." % (vertex, distances[vertex])


if __name__ == '__main__':
    main()
