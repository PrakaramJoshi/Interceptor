#pragma once
namespace Interceptor {
	const static std::string html_data_key = "$CALL_GRAPH_DATA$";
	const static std::string html_package_names_key = "$PACKAGE_NAME$";
	const static std::string html_dependency_matrix_key = "$DEPENDENCY_MATRIX$";
	const static std::string html_dependency_wheel_placeholder_key = "$PLACEHOLDER$";
	const static std::string html_dependency_wheel_chart_key = "$CHART$";
	const static std::string html_dependency_wheel_data_key = "$DATA$";
	const static std::string html_dependency_wheel_all_charts_key = "$ALL_CHARTS$";
	const static std::string html_dependency_wheel_char_placeholder_key = "$CHART_PLACEHOLDER$";
	const static std::string html_dependency_wheel_thread_key = "$THREAD$";
	const static std::string html_dependency_wheel_colspan_key = "$COLSPAN$";
	const static std::string html_dependency_wheel_chart_script = R"( var $CHART$ = d3.chart.dependencyWheel();
  var $DATA$ = {
	packageNames: [$PACKAGE_NAME$],
	matrix : [$DEPENDENCY_MATRIX$]
  };
  d3.select('#$CHART_PLACEHOLDER$')
	  .datum($DATA$)
	  .call($CHART$); )";
	const static std::string html_dependency_wheel_chart_placholder = R"( <td id="$CHART_PLACEHOLDER$" colspan="$COLSPAN$">$THREAD$</td>)";
	const static std::string html_call_graph_force_diagram = R"(<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html lang="en">
	<head>
		<meta charset="utf-8">
		<title>Force based label placement</title>
		<script type="text/javascript" src="http://mbostock.github.com/d3/d3.js?2.6.0"></script>
		<script type="text/javascript" src="http://mbostock.github.com/d3/d3.layout.js?2.6.0"></script>
		<script type="text/javascript" src="http://mbostock.github.com/d3/d3.geom.js?2.6.0"></script>
	</head>
	<body>
<script type="text/javascript" charset="utf-8">
			var w =1200, h = 1200;

							var labelDistance = 0;

							var vis = d3.select("body").append("svg:svg").attr("width", w).attr("height", h);

							var nodes_index = []
			var nodes = [];
			var labelAnchors = [];
			var labelAnchorLinks = [];
			var links = [];
			console.log("here")
			var read_data = function(){
				var reader =  $CALL_GRAPH_DATA$
				var text = reader.split("|");
				var i=1;
				for(; i < parseInt(text[0])+1; i++) {
					nodes_index.push(text[i]);
					var node = {
						label : text[i]
					};
					nodes.push(node);
					labelAnchors.push({
						node : node
					});
					labelAnchors.push({
						node : node
					});
				};
				for(;i<text.length;i++){
					var link_info = text[i].split(";")
					if(link_info!=''){
						var src = nodes_index.indexOf(link_info[0])
						console.log("link_info: "+link_info)
						console.log("src "+src)
						for(var j=1;j<link_info.length;){
							var targ = nodes_index.indexOf(link_info[j])
							var w = parseFloat(link_info[j+1]);
							j=j+2
							links.push({
								source : src,
								target : targ,
								weight : w
							});
						}
						labelAnchorLinks.push({
							source : src*2,
							target : src*2 + 1,
							weight : 1
						});
					}

								}
				var print = function(object){
					var output = '';
					for (var property in object) {
					  output += property + ': ' + object[property]+'; ';
					}
					return output
				}
				var print_array = function(name,arr){
					var array_str = '';
					for(var i=0;i<arr.length;i++){
						array_str =array_str + print(arr[i])
					}
					console.log(name +" : "+array_str )
				}
				print_array("node",nodes)
				print_array("labelAnchorLinks",labelAnchorLinks)
				print_array("links",links)
			}
			read_data();

							var force = d3.layout.force().size([w, h]).nodes(nodes).links(links).gravity(1).linkDistance(50).charge(-3000).linkStrength(function(x) {
				return x.weight * 10
			});
			force.start();

							var force2 = d3.layout.force().nodes(labelAnchors).links(labelAnchorLinks).gravity(0).linkDistance(0).linkStrength(0.8).charge(-100).size([w, h]);
			force2.start();

							var link = vis.selectAll("line.link").data(links).enter().append("svg:line").attr("class", "link").style("stroke", "#CCC");

							var node = vis.selectAll("g.node").data(force.nodes()).enter().append("svg:g").attr("class", "node");
			node.append("svg:circle").attr("r", 5).style("fill", "#555").style("stroke", "#FFF").style("stroke-width", 3);
			node.call(force.drag);


							var anchorLink = vis.selectAll("line.anchorLink").data(labelAnchorLinks)//.enter().append("svg:line").attr("class", "anchorLink").style("stroke", "#999");

							var anchorNode = vis.selectAll("g.anchorNode").data(force2.nodes()).enter().append("svg:g").attr("class", "anchorNode");
			anchorNode.append("svg:circle").attr("r", 0).style("fill", "#FFF");
				anchorNode.append("svg:text").text(function(d, i) {
				return i % 2 == 0 ? "" : d.node.label
			}).style("fill", "#555").style("font-family", "Arial").style("font-size", 12);

							var updateLink = function() {
				this.attr("x1", function(d) {
					return d.source.x;
				}).attr("y1", function(d) {
					return d.source.y;
				}).attr("x2", function(d) {
					return d.target.x;
				}).attr("y2", function(d) {
					return d.target.y;
				});

							}

							var updateNode = function() {
				this.attr("transform", function(d) {
					return "translate(" + d.x + "," + d.y + ") ";
});

}


force.on("tick", function() {

	force2.start();

					node.call(updateNode);

					anchorNode.each(function(d, i) {
		if (i % 2 == 0) {
			d.x = d.node.x;
			d.y = d.node.y;
		}
		else {
			var b = this.childNodes[1].getBBox();

							var diffX = d.x - d.node.x;
			var diffY = d.y - d.node.y;

							var dist = Math.sqrt(diffX * diffX + diffY * diffY);

							var shiftX = b.width * (diffX - dist) / (dist * 2);
			shiftX = Math.max(-b.width, Math.min(0, shiftX));
			var shiftY = 5;
			this.childNodes[1].setAttribute("transform", "translate(" + shiftX + "," + shiftY + ") ");
		}
	});


					anchorNode.call(updateNode);

					link.call(updateLink);
	anchorLink.call(updateLink);

				});

				</script>
</body>
</html>

				)";
	const static std::string html_call_dependency= R"(<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en-us" prefix="og: http://ogp.me/ns#">
  <!-- Use the Source, Luke -->
  <head>
    <title>DependencyWheel: An Interactive Visualization Of Package Dependencies</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta charset="utf-8">
  <style>
#chart_placeholder0 {
  text-align: center;
  margin-bottom: 20px;
}
.dependencyWheel {
  font: 20px sans-serif;
}
table#t01 tr:nth-child(even) {
    background-color: #aaa;
}
table#t01 tr:nth-child(odd) {
    background-color: #fff;
}
table#t01 th {
    color: white;
    background-color: black;
} 
table, th, td {
    border: 2px solid black;
    border-collapse: collapse;
}
</style>
</head>
<body>
  <div class="container">
<table id="t01" style="width:100%">
  $PLACEHOLDER$
</table>
</div>
  <script type="text/javascript" src="http://mbostock.github.com/d3/d3.js"></script>
  <script>
    d3.chart = d3.chart || {};
    d3.chart.dependencyWheel = function(options) {

    var width = 1000;
    var margin = 150;
    var padding = 0.04;

    function chart(selection) {
      selection.each(function(data) {

        var matrix = data.matrix;
        var packageNames = data.packageNames;
        var radius = width / 2 - margin;

        // create the layout
        var chord = d3.layout.chord()
          .padding(padding)
          .sortSubgroups(d3.descending);

        // Select the svg element, if it exists.
        var svg = d3.select(this).selectAll("svg").data([data]);

        // Otherwise, create the skeletal chart.
        var gEnter = svg.enter().append("svg:svg")
          .attr("width", width)
          .attr("height", width)
          .attr("class", "dependencyWheel")
        .append("g")
          .attr("transform", "translate(" + (width / 2) + "," + (width / 2) + ") ");

		var arc = d3.svg.arc()
		.innerRadius(radius)
		.outerRadius(radius + 20);

	var fill = function(d) {
		if (d.index === 0) return '#ccc';
		var str = packageNames[d.index].toLowerCase();
		var n = str.length;
		var total=0;
		for(var j=0;j<n;j++){
			total = total +((parseInt(str.charCodeAt(j)-97)/26));
		}
		total=total/n;
		return "hsl(" + parseInt(total* 360, 10) + ",90%,70%) ";
	};

	// Returns an event handler for fading a given chord group.
	var fade = function(opacity) {
		return function(g, i) {
			svg.selectAll(".chord")
				.filter(function(d) {
				return d.source.index != i && d.target.index != i;
			})
				.transition()
				.style("opacity", opacity);
			var groups = [];
			svg.selectAll(".chord")
				.filter(function(d) {
				if (d.source.index == i) {
					groups.push(d.target.index);
				}
				if (d.target.index == i) {
					groups.push(d.source.index);
				}
			});
			groups.push(i);
			var length = groups.length;
			svg.selectAll('.group')
				.filter(function(d) {
				for (var i = 0; i < length; i++) {
					if (groups[i] == d.index) return false;
				}
				return true;
			})
				.transition()
				.style("opacity", opacity);
		};
	};

	chord.matrix(matrix);

	var rootGroup = chord.groups()[0];
	var rotation = -(rootGroup.endAngle - rootGroup.startAngle) / 2 * (180 / Math.PI);

	var g = gEnter.selectAll("g.group")
		.data(chord.groups)
		.enter().append("svg:g")
		.attr("class", "group")
		.attr("transform", function(d) {
		return "rotate(" + rotation + ") ";
	});

	g.append("svg:path")
		.style("fill", fill)
		.style("stroke", fill)
		.attr("d", arc)
		.style("cursor", "pointer")
		.on("mouseover", fade(0.1))
		.on("mouseout", fade(1));

	g.append("svg:text")
		.each(function(d) { d.angle = (d.startAngle + d.endAngle) / 2; })
		.attr("dy", ".35em")
		.attr("text-anchor", function(d) { return d.angle > Math.PI ? "end" : null; })
		.attr("transform", function(d) {
		return "rotate(" + (d.angle * 180 / Math.PI - 90) + ") " +
			"translate(" + (radius + 26) + ") " +
			(d.angle > Math.PI ? "rotate(180) " : "");
	})
		.style("cursor", "pointer")
		.text(function(d) { return packageNames[d.index]; })
		.on("mouseover", fade(0.1))
		.on("mouseout", fade(1));

	gEnter.selectAll("path.chord")
		.data(chord.chords)
		.enter().append("svg:path")
		.attr("class", "chord")
		.style("stroke", function(d) { return d3.rgb(fill(d.source)).darker(); })
		.style("fill", function(d) { return fill(d.source); })
		.attr("d", d3.svg.chord().radius(radius))
		.attr("transform", function(d) {
		return "rotate(" + rotation + ") ";
	})
		.style("opacity", 1);
	  });
	}

	chart.width = function(value) {
		if (!arguments.length) return width;
		width = value;
		return chart;
	};

	chart.margin = function(value) {
		if (!arguments.length) return margin;
		margin = value;
		return chart;
	};

	chart.padding = function(value) {
		if (!arguments.length) return padding;
		padding = value;
		return chart;
	};

	return chart;
  };
	
  $ALL_CHARTS$

  </script>
	  </body>
	  </html>
	  )";
};