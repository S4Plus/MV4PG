match (n0:Place{name:$placeName})<-[r1:isPartOf]-(n1:Place)
match (n0)<-[r2*2]-(n2:Comment) return count(n2)