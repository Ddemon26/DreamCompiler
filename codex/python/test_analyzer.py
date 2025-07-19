#!/usr/bin/env python3
"""
DreamCompiler Test Analysis and Reporting Tool
Provides detailed analysis of test results across platforms and time
"""

import argparse
import json
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Optional
import sys

# Configuration
ROOT = Path(__file__).resolve().parent.parent.parent
RESULTS_DIR = ROOT / "test_results"

class TestAnalyzer:
    """Analyze test results and generate reports"""
    
    def __init__(self):
        self.results_dir = RESULTS_DIR
        
    def load_results(self, pattern: str = "*.json") -> List[Dict]:
        """Load all test result files matching pattern"""
        result_files = list(self.results_dir.glob(pattern))
        results = []
        
        for file_path in result_files:
            try:
                with open(file_path, 'r') as f:
                    data = json.load(f)
                    data['file_path'] = str(file_path)
                    results.append(data)
            except Exception as e:
                print(f"Error loading {file_path}: {e}")
                
        return sorted(results, key=lambda x: x['timestamp'])
        
    def generate_summary_report(self, results: List[Dict]) -> str:
        """Generate comprehensive summary report"""
        if not results:
            return "No test results found."
            
        report = []
        report.append("DREAMCOMPILER TEST ANALYSIS REPORT")
        report.append("=" * 50)
        report.append("")
        
        # Overall statistics
        total_suites = len(results)
        platforms = set(r['platform'] for r in results)
        date_range = self._get_date_range(results)
        
        report.append(f"Analysis Period: {date_range}")
        report.append(f"Total Test Suites: {total_suites}")
        report.append(f"Platforms: {', '.join(platforms)}")
        report.append("")
        
        # Platform comparison
        report.append("PLATFORM COMPARISON")
        report.append("-" * 20)
        
        platform_stats = {}
        for platform in platforms:
            platform_results = [r for r in results if r['platform'] == platform]
            if platform_results:
                latest = platform_results[-1]  # Most recent
                platform_stats[platform] = {
                    'total': len(latest['results']),
                    'passed': sum(1 for r in latest['results'] if r['status'] == 'PASS'),
                    'failed': sum(1 for r in latest['results'] if r['status'] == 'FAIL'),
                    'pass_rate': 0
                }
                if platform_stats[platform]['total'] > 0:
                    platform_stats[platform]['pass_rate'] = (
                        platform_stats[platform]['passed'] / platform_stats[platform]['total'] * 100
                    )
                    
        for platform, stats in platform_stats.items():
            report.append(f"{platform:10} {stats['passed']:3}/{stats['total']:3} tests passed ({stats['pass_rate']:5.1f}%)")
            
        report.append("")
        
        # Trend analysis
        if len(results) > 1:
            report.append("TREND ANALYSIS")
            report.append("-" * 15)
            
            for platform in platforms:
                platform_results = [r for r in results if r['platform'] == platform]
                if len(platform_results) >= 2:
                    old_pass_rate = self._calculate_pass_rate(platform_results[0])
                    new_pass_rate = self._calculate_pass_rate(platform_results[-1])
                    trend = new_pass_rate - old_pass_rate
                    
                    trend_symbol = "↑" if trend > 0 else "↓" if trend < 0 else "→"
                    report.append(f"{platform:10} {trend_symbol} {trend:+5.1f}% change")
                    
            report.append("")
            
        # Failing tests analysis
        report.append("CONSISTENTLY FAILING TESTS")
        report.append("-" * 27)
        
        failing_tests = self._find_consistently_failing_tests(results)
        if failing_tests:
            for test_path, failure_count in failing_tests.items():
                report.append(f"  {test_path} (failed {failure_count}/{total_suites} times)")
        else:
            report.append("  No consistently failing tests found.")
            
        report.append("")
        
        # Performance analysis
        report.append("PERFORMANCE ANALYSIS")
        report.append("-" * 20)
        
        perf_stats = self._analyze_performance(results)
        for platform, stats in perf_stats.items():
            report.append(f"{platform}:")
            report.append(f"  Avg compile time: {stats['avg_compile_time']:.3f}s")
            report.append(f"  Avg runtime:      {stats['avg_runtime']:.3f}s")
            report.append(f"  Slowest test:     {stats['slowest_test']}")
            
        return "\n".join(report)
        
    def _get_date_range(self, results: List[Dict]) -> str:
        """Get human-readable date range from results"""
        if not results:
            return "No data"
            
        timestamps = [r['timestamp'] for r in results]
        start_date = datetime.fromtimestamp(min(timestamps))
        end_date = datetime.fromtimestamp(max(timestamps))
        
        if start_date.date() == end_date.date():
            return start_date.strftime("%Y-%m-%d")
        else:
            return f"{start_date.strftime('%Y-%m-%d')} to {end_date.strftime('%Y-%m-%d')}"
            
    def _calculate_pass_rate(self, result: Dict) -> float:
        """Calculate pass rate for a test result"""
        if not result['results']:
            return 0.0
            
        passed = sum(1 for r in result['results'] if r['status'] == 'PASS')
        total = len(result['results'])
        return (passed / total * 100) if total > 0 else 0.0
        
    def _find_consistently_failing_tests(self, results: List[Dict]) -> Dict[str, int]:
        """Find tests that fail consistently across runs"""
        test_failures = {}
        
        for result in results:
            for test in result['results']:
                if test['status'] == 'FAIL':
                    path = test['path']
                    test_failures[path] = test_failures.get(path, 0) + 1
                    
        # Return tests that fail in more than 50% of runs
        threshold = len(results) * 0.5
        return {path: count for path, count in test_failures.items() if count > threshold}
        
    def _analyze_performance(self, results: List[Dict]) -> Dict[str, Dict]:
        """Analyze performance metrics across platforms"""
        perf_stats = {}
        
        for result in results:
            platform = result['platform']
            if platform not in perf_stats:
                perf_stats[platform] = {
                    'compile_times': [],
                    'runtimes': [],
                    'test_names': []
                }
                
            for test in result['results']:
                if test['status'] in ['PASS', 'FAIL']:  # Only include executed tests
                    perf_stats[platform]['compile_times'].append(test['compile_time'])
                    perf_stats[platform]['runtimes'].append(test['runtime'])
                    perf_stats[platform]['test_names'].append(test['name'])
                    
        # Calculate averages and find slowest tests
        for platform, stats in perf_stats.items():
            if stats['compile_times']:
                stats['avg_compile_time'] = sum(stats['compile_times']) / len(stats['compile_times'])
                stats['avg_runtime'] = sum(stats['runtimes']) / len(stats['runtimes'])
                
                # Find slowest test
                total_times = [c + r for c, r in zip(stats['compile_times'], stats['runtimes'])]
                slowest_idx = total_times.index(max(total_times))
                stats['slowest_test'] = stats['test_names'][slowest_idx]
            else:
                stats['avg_compile_time'] = 0.0
                stats['avg_runtime'] = 0.0
                stats['slowest_test'] = "N/A"
                
        return perf_stats
        
    def generate_html_report(self, results: List[Dict], output_path: Path):
        """Generate interactive HTML report"""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>DreamCompiler Test Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .section {{ margin: 20px 0; }}
        .platform {{ background-color: #e8f4f8; padding: 15px; margin: 10px 0; border-radius: 5px; }}
        .test-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 15px; }}
        .test-card {{ border: 1px solid #ddd; padding: 15px; border-radius: 5px; }}
        .pass {{ border-left: 5px solid #4CAF50; }}
        .fail {{ border-left: 5px solid #f44336; }}
        .skip {{ border-left: 5px solid #ff9800; }}
        .error {{ border-left: 5px solid #9c27b0; }}
        table {{ width: 100%; border-collapse: collapse; }}
        th, td {{ padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #f2f2f2; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>DreamCompiler Test Report</h1>
        <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
    
    <div class="section">
        <h2>Summary</h2>
        <pre>{self.generate_summary_report(results)}</pre>
    </div>
    
    <div class="section">
        <h2>Detailed Results</h2>
        {self._generate_detailed_html(results)}
    </div>
</body>
</html>
"""
        
        with open(output_path, 'w') as f:
            f.write(html_content)
            
    def _generate_detailed_html(self, results: List[Dict]) -> str:
        """Generate detailed HTML for test results"""
        html_parts = []
        
        for result in results[-3:]:  # Show last 3 runs
            timestamp = datetime.fromtimestamp(result['timestamp']).strftime('%Y-%m-%d %H:%M:%S')
            html_parts.append(f"""
            <div class="platform">
                <h3>{result['platform']} - {timestamp}</h3>
                <p>Duration: {result['total_duration']:.2f}s</p>
                
                <div class="test-grid">
            """)
            
            for test in result['results']:
                status_class = test['status'].lower()
                html_parts.append(f"""
                <div class="test-card {status_class}">
                    <h4>{test['name']}</h4>
                    <p><strong>Status:</strong> {test['status']}</p>
                    <p><strong>Category:</strong> {test['category']}</p>
                    <p><strong>Duration:</strong> {test['duration']:.3f}s</p>
                    {f"<p><strong>Error:</strong> {test['error_message']}</p>" if test.get('error_message') else ""}
                </div>
                """)
                
            html_parts.append("</div></div>")
            
        return "".join(html_parts)
        
    def compare_platforms(self, results: List[Dict]):
        """Generate platform comparison charts"""
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns
            
            # Prepare data for visualization
            platform_data = []
            for result in results:
                platform_data.append({
                    'platform': result['platform'],
                    'timestamp': result['timestamp'],
                    'pass_rate': self._calculate_pass_rate(result),
                    'total_tests': len(result['results']),
                    'duration': result['total_duration']
                })
                
            df = pd.DataFrame(platform_data)
            
            # Create subplots
            fig, axes = plt.subplots(2, 2, figsize=(15, 10))
            fig.suptitle('DreamCompiler Test Analysis', fontsize=16)
            
            # Pass rate comparison
            sns.boxplot(data=df, x='platform', y='pass_rate', ax=axes[0, 0])
            axes[0, 0].set_title('Pass Rate by Platform')
            axes[0, 0].set_ylabel('Pass Rate (%)')
            
            # Test duration comparison
            sns.boxplot(data=df, x='platform', y='duration', ax=axes[0, 1])
            axes[0, 1].set_title('Test Suite Duration by Platform')
            axes[0, 1].set_ylabel('Duration (seconds)')
            
            # Pass rate over time
            for platform in df['platform'].unique():
                platform_df = df[df['platform'] == platform]
                axes[1, 0].plot(platform_df['timestamp'], platform_df['pass_rate'], 
                              marker='o', label=platform)
            axes[1, 0].set_title('Pass Rate Trend Over Time')
            axes[1, 0].set_ylabel('Pass Rate (%)')
            axes[1, 0].legend()
            
            # Test count comparison
            sns.barplot(data=df, x='platform', y='total_tests', ax=axes[1, 1])
            axes[1, 1].set_title('Total Tests by Platform')
            axes[1, 1].set_ylabel('Number of Tests')
            
            plt.tight_layout()
            
            # Save chart
            chart_path = self.results_dir / "test_analysis_charts.png"
            plt.savefig(chart_path, dpi=300, bbox_inches='tight')
            print(f"Charts saved to {chart_path}")
            
        except ImportError:
            print("matplotlib and seaborn required for chart generation")
            print("Install with: pip install matplotlib seaborn pandas")

def main():
    parser = argparse.ArgumentParser(description="DreamCompiler Test Analyzer")
    parser.add_argument("--pattern", default="*.json", help="Result file pattern")
    parser.add_argument("--format", choices=["text", "html", "charts"], default="text", 
                       help="Output format")
    parser.add_argument("--output", help="Output file path")
    parser.add_argument("--days", type=int, help="Analyze results from last N days")
    
    args = parser.parse_args()
    
    analyzer = TestAnalyzer()
    results = analyzer.load_results(args.pattern)
    
    if not results:
        print("No test results found.")
        return
        
    # Filter by date if specified
    if args.days:
        cutoff_time = datetime.now().timestamp() - (args.days * 24 * 3600)
        results = [r for r in results if r['timestamp'] >= cutoff_time]
        
    if args.format == "text":
        report = analyzer.generate_summary_report(results)
        if args.output:
            with open(args.output, 'w') as f:
                f.write(report)
            print(f"Report saved to {args.output}")
        else:
            print(report)
            
    elif args.format == "html":
        output_path = Path(args.output) if args.output else RESULTS_DIR / "test_report.html"
        analyzer.generate_html_report(results, output_path)
        print(f"HTML report saved to {output_path}")
        
    elif args.format == "charts":
        analyzer.compare_platforms(results)

if __name__ == "__main__":
    main()