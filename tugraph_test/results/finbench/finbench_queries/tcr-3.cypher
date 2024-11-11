不使用
MATCH (src:Account{id:%d}), (dst:Account{id:%d}) 
CALL algo.shortestPath( src, dst, { direction: 'PointingRight', relationshipQuery:'transfer', edgeFilter: { timestamp: { smaller_than: %d, greater_than: %d } } } ) 
YIELD nodeCount RETURN nodeCount - 1 AS len;