match (n0:Place{name:$placeName})<-[r1]-(n1)<-[r2]-(n2:Person)
match (n0)<-[r3*2]-(n3:Comment) return count(n3)