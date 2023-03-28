console.time('total');
let str = '';
process.stdin.on('data', (data) => {
	str += data;
})

process.stdin.on('end', () => {
	console.time('measure');
	const obj = JSON.parse(str);
	//console.log(obj);
	console.timeEnd('measure');
})
