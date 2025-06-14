#!/usr/bin/env python3
"""
MINIX4 Advanced Code Analysis Suite
Comprehensive technical debt and quality analysis using modern Python tools
"""

import os
import sys
import json
import subprocess
import threading
import concurrent.futures
from pathlib import Path
from dataclasses import dataclass, asdict
from typing import Dict, List, Optional, Tuple
import argparse
import time

@dataclass
class AnalysisResult:
    """Results from a single analysis tool"""
    tool: str
    status: str
    duration: float
    output_file: Optional[str] = None
    metrics: Optional[Dict] = None
    errors: Optional[List[str]] = None

class CodeAnalysisSuite:
    """Advanced code analysis suite for MINIX4"""
    
    def __init__(self, workspace_root: str, output_dir: str):
        self.workspace_root = Path(workspace_root)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.results = {}
        
    def check_tool_availability(self) -> Dict[str, bool]:
        """Check which analysis tools are available"""
        tools = {
            'lizard': 'lizard --version',
            'radon': 'radon --version',
            'vulture': 'vulture --version',
            'bandit': 'bandit --version',
            'semgrep': 'semgrep --version',
            'flawfinder': 'flawfinder --version',
            'cppcheck': 'cppcheck --version',
            'cloc': 'cloc --version',
            'cscope': 'cscope -V',
            'git': 'git --version'
        }
        
        available = {}
        for tool, cmd in tools.items():
            try:
                result = subprocess.run(cmd.split(), capture_output=True, timeout=10)
                available[tool] = result.returncode == 0
            except (subprocess.TimeoutExpired, FileNotFoundError):
                available[tool] = False
                
        return available
    
    def run_complexity_analysis(self) -> AnalysisResult:
        """Run cyclomatic complexity analysis with Lizard"""
        start_time = time.time()
        output_file = self.output_dir / "complexity_analysis.json"
        
        try:
            cmd = [
                'lizard',
                str(self.workspace_root),
                '--language', 'cpp',
                '--output_file', str(output_file),
                '--format', 'json',
                '--sort', 'cyclomatic_complexity',
                '--threshold_cyclomatic_complexity', '15',
                '--threshold_length', '1000',
                '--threshold_parameter_count', '5'
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            
            # Parse results for metrics
            metrics = {}
            if output_file.exists():
                with open(output_file) as f:
                    data = json.load(f)
                    metrics = {
                        'total_functions': len(data.get('function_list', [])),
                        'complex_functions': len([f for f in data.get('function_list', []) 
                                                if f.get('cyclomatic_complexity', 0) > 15]),
                        'average_complexity': sum(f.get('cyclomatic_complexity', 0) 
                                                for f in data.get('function_list', [])) / 
                                            max(len(data.get('function_list', [])), 1),
                        'total_lines': data.get('total_lines_of_code', 0)
                    }
            
            return AnalysisResult(
                tool='lizard',
                status='success' if result.returncode == 0 else 'failed',
                duration=time.time() - start_time,
                output_file=str(output_file) if output_file.exists() else None,
                metrics=metrics,
                errors=[result.stderr] if result.stderr else None
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='lizard',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_security_analysis(self) -> AnalysisResult:
        """Run security vulnerability analysis"""
        start_time = time.time()
        output_file = self.output_dir / "security_analysis.json"
        
        try:
            # Use flawfinder for C/C++ security analysis
            cmd = [
                'flawfinder',
                '--quiet',
                '--dataonly',
                '--html',
                '--output', str(output_file.with_suffix('.html')),
                str(self.workspace_root)
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
            
            # Also run with text output for parsing
            cmd_text = [
                'flawfinder',
                '--quiet',
                '--dataonly',
                str(self.workspace_root)
            ]
            
            result_text = subprocess.run(cmd_text, capture_output=True, text=True, timeout=600)
            
            # Parse results
            metrics = {}
            if result_text.stdout:
                lines = result_text.stdout.split('\n')
                metrics = {
                    'total_issues': len([l for l in lines if '[' in l and ']' in l]),
                    'high_risk': len([l for l in lines if '[5]' in l]),
                    'medium_risk': len([l for l in lines if '[3]' in l or '[4]' in l]),
                    'low_risk': len([l for l in lines if '[1]' in l or '[2]' in l])
                }
            
            return AnalysisResult(
                tool='flawfinder',
                status='success' if result.returncode == 0 else 'failed',
                duration=time.time() - start_time,
                output_file=str(output_file.with_suffix('.html')),
                metrics=metrics,
                errors=[result.stderr] if result.stderr else None
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='flawfinder',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_duplication_analysis(self) -> AnalysisResult:
        """Run code duplication analysis"""
        start_time = time.time()
        output_file = self.output_dir / "duplication_analysis.json"
        
        try:
            # Use jscpd for duplicate detection
            cmd = [
                'jscpd',
                str(self.workspace_root),
                '--reporters', 'json',
                '--output', str(self.output_dir),
                '--format', 'c,cpp,h',
                '--min-lines', '5',
                '--min-tokens', '50',
                '--threshold', '1'
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=900)
            
            # Parse results
            metrics = {}
            jscpd_output = self.output_dir / "jscpd-report.json"
            if jscpd_output.exists():
                with open(jscpd_output) as f:
                    data = json.load(f)
                    duplicates = data.get('duplicates', [])
                    metrics = {
                        'total_duplicates': len(duplicates),
                        'duplicate_lines': sum(d.get('lines', 0) for d in duplicates),
                        'duplicate_tokens': sum(d.get('tokens', 0) for d in duplicates),
                        'duplication_percentage': data.get('statistics', {}).get('percentage', 0)
                    }
            
            return AnalysisResult(
                tool='jscpd',
                status='success' if result.returncode == 0 else 'failed',
                duration=time.time() - start_time,
                output_file=str(jscpd_output) if jscpd_output.exists() else None,
                metrics=metrics,
                errors=[result.stderr] if result.stderr else None
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='jscpd',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_static_analysis(self) -> AnalysisResult:
        """Run comprehensive static analysis with cppcheck"""
        start_time = time.time()
        output_file = self.output_dir / "static_analysis.xml"
        
        try:
            cmd = [
                'cppcheck',
                '--enable=all',
                '--xml',
                '--xml-version=2',
                '--output-file=' + str(output_file),
                '--platform=native',
                '--std=c23',
                '--verbose',
                '--force',
                str(self.workspace_root)
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)  # 30 minutes
            
            # Parse XML results (simplified)
            metrics = {}
            if output_file.exists():
                import xml.etree.ElementTree as ET
                try:
                    tree = ET.parse(output_file)
                    root = tree.getroot()
                    errors = root.findall('.//error')
                    
                    severities = {}
                    for error in errors:
                        severity = error.get('severity', 'unknown')
                        severities[severity] = severities.get(severity, 0) + 1
                    
                    metrics = {
                        'total_issues': len(errors),
                        'by_severity': severities,
                        'files_analyzed': len(set(e.get('file', '') for e in errors))
                    }
                except Exception as parse_error:
                    metrics = {'parse_error': str(parse_error)}
            
            return AnalysisResult(
                tool='cppcheck',
                status='success' if result.returncode == 0 else 'failed',
                duration=time.time() - start_time,
                output_file=str(output_file) if output_file.exists() else None,
                metrics=metrics,
                errors=[result.stderr] if result.stderr else None
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='cppcheck',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_line_count_analysis(self) -> AnalysisResult:
        """Run detailed line count analysis with cloc"""
        start_time = time.time()
        output_file = self.output_dir / "line_count_analysis.json"
        
        try:
            cmd = [
                'cloc',
                '--json',
                '--out=' + str(output_file),
                '--exclude-dir=.git,node_modules,build,__pycache__',
                '--include-lang=C,C++,C/C++ Header',
                str(self.workspace_root)
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            
            # Parse results
            metrics = {}
            if output_file.exists():
                with open(output_file) as f:
                    data = json.load(f)
                    
                    total_stats = data.get('SUM', {})
                    metrics = {
                        'total_files': total_stats.get('nFiles', 0),
                        'blank_lines': total_stats.get('blank', 0),
                        'comment_lines': total_stats.get('comment', 0),
                        'source_lines': total_stats.get('code', 0),
                        'languages': {k: v for k, v in data.items() 
                                    if k not in ['header', 'SUM'] and isinstance(v, dict)}
                    }
            
            return AnalysisResult(
                tool='cloc',
                status='success' if result.returncode == 0 else 'failed',
                duration=time.time() - start_time,
                output_file=str(output_file) if output_file.exists() else None,
                metrics=metrics,
                errors=[result.stderr] if result.stderr else None
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='cloc',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_git_analysis(self) -> AnalysisResult:
        """Run Git repository analysis for hotspots and churn"""
        start_time = time.time()
        output_file = self.output_dir / "git_analysis.json"
        
        try:
            # Get file churn data
            cmd_churn = [
                'git', 'log', '--name-only', '--pretty=format:', '--since=1.year.ago'
            ]
            
            churn_result = subprocess.run(cmd_churn, capture_output=True, text=True, 
                                        cwd=self.workspace_root, timeout=60)
            
            # Get commit statistics
            cmd_stats = [
                'git', 'log', '--shortstat', '--since=1.year.ago', '--pretty=format:%h'
            ]
            
            stats_result = subprocess.run(cmd_stats, capture_output=True, text=True,
                                        cwd=self.workspace_root, timeout=60)
            
            # Parse results
            metrics = {}
            if churn_result.stdout:
                files = [f for f in churn_result.stdout.split('\n') if f.strip()]
                file_counts = {}
                for f in files:
                    file_counts[f] = file_counts.get(f, 0) + 1
                
                # Top 20 most changed files
                top_files = sorted(file_counts.items(), key=lambda x: x[1], reverse=True)[:20]
                
                metrics = {
                    'total_file_changes': len(files),
                    'unique_files_changed': len(file_counts),
                    'top_changed_files': top_files,
                    'average_changes_per_file': len(files) / max(len(file_counts), 1)
                }
            
            # Save results
            with open(output_file, 'w') as f:
                json.dump(metrics, f, indent=2)
            
            return AnalysisResult(
                tool='git',
                status='success',
                duration=time.time() - start_time,
                output_file=str(output_file),
                metrics=metrics
            )
            
        except Exception as e:
            return AnalysisResult(
                tool='git',
                status='error',
                duration=time.time() - start_time,
                errors=[str(e)]
            )
    
    def run_all_analyses(self, max_workers: int = 4) -> Dict[str, AnalysisResult]:
        """Run all analyses in parallel"""
        print("Starting comprehensive code analysis suite...")
        
        # Check tool availability
        available_tools = self.check_tool_availability()
        print(f"Available tools: {[k for k, v in available_tools.items() if v]}")
        print(f"Missing tools: {[k for k, v in available_tools.items() if not v]}")
        
        # Define analysis tasks
        analyses = []
        if available_tools.get('lizard', False):
            analyses.append(('complexity', self.run_complexity_analysis))
        if available_tools.get('flawfinder', False):
            analyses.append(('security', self.run_security_analysis))
        if available_tools.get('cppcheck', False):
            analyses.append(('static', self.run_static_analysis))
        if available_tools.get('cloc', False):
            analyses.append(('linecount', self.run_line_count_analysis))
        if available_tools.get('git', False):
            analyses.append(('git', self.run_git_analysis))
        
        # Add duplication analysis (using jscpd via npm/npx if available)
        analyses.append(('duplication', self.run_duplication_analysis))
        
        print(f"Running {len(analyses)} analyses with {max_workers} workers...")
        
        # Run analyses in parallel
        results = {}
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            future_to_analysis = {executor.submit(func): name for name, func in analyses}
            
            for future in concurrent.futures.as_completed(future_to_analysis):
                analysis_name = future_to_analysis[future]
                try:
                    result = future.result()
                    results[analysis_name] = result
                    print(f"✓ {analysis_name} completed in {result.duration:.1f}s")
                except Exception as e:
                    print(f"✗ {analysis_name} failed: {e}")
                    results[analysis_name] = AnalysisResult(
                        tool=analysis_name,
                        status='error',
                        duration=0,
                        errors=[str(e)]
                    )
        
        self.results = results
        return results
    
    def generate_comprehensive_report(self) -> str:
        """Generate a comprehensive HTML report"""
        report_file = self.output_dir / "comprehensive_analysis_report.html"
        
        # Calculate summary metrics
        total_duration = sum(r.duration for r in self.results.values())
        successful_analyses = len([r for r in self.results.values() if r.status == 'success'])
        total_analyses = len(self.results)
        
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>MINIX4 Comprehensive Code Analysis Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background: #2c3e50; color: white; padding: 20px; border-radius: 5px; }}
        .summary {{ background: #ecf0f1; padding: 15px; margin: 20px 0; border-radius: 5px; }}
        .analysis {{ margin: 20px 0; padding: 15px; border: 1px solid #bdc3c7; border-radius: 5px; }}
        .success {{ border-left: 5px solid #27ae60; }}
        .failed {{ border-left: 5px solid #e74c3c; }}
        .error {{ border-left: 5px solid #e67e22; }}
        .metrics {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 10px; }}
        .metric {{ background: #f8f9fa; padding: 10px; border-radius: 3px; }}
        pre {{ background: #2c3e50; color: #ecf0f1; padding: 10px; border-radius: 3px; overflow-x: auto; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>MINIX4 Comprehensive Code Analysis Report</h1>
        <p>Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p>Workspace: {self.workspace_root}</p>
    </div>
    
    <div class="summary">
        <h2>Summary</h2>
        <div class="metrics">
            <div class="metric">
                <strong>Total Analyses:</strong> {total_analyses}
            </div>
            <div class="metric">
                <strong>Successful:</strong> {successful_analyses}
            </div>
            <div class="metric">
                <strong>Total Duration:</strong> {total_duration:.1f} seconds
            </div>
            <div class="metric">
                <strong>Success Rate:</strong> {(successful_analyses/total_analyses*100):.1f}%
            </div>
        </div>
    </div>
"""
        
        # Add individual analysis results
        for name, result in self.results.items():
            status_class = result.status
            html_content += f"""
    <div class="analysis {status_class}">
        <h3>{result.tool.title()} Analysis</h3>
        <p><strong>Status:</strong> {result.status}</p>
        <p><strong>Duration:</strong> {result.duration:.2f} seconds</p>
"""
            
            if result.metrics:
                html_content += "<h4>Metrics:</h4><div class='metrics'>"
                for key, value in result.metrics.items():
                    if isinstance(value, (int, float)):
                        html_content += f"<div class='metric'><strong>{key.replace('_', ' ').title()}:</strong> {value:,}</div>"
                    elif isinstance(value, dict) and len(value) < 10:
                        html_content += f"<div class='metric'><strong>{key.replace('_', ' ').title()}:</strong><br>"
                        for k, v in value.items():
                            html_content += f"{k}: {v}<br>"
                        html_content += "</div>"
                html_content += "</div>"
            
            if result.output_file:
                html_content += f"<p><strong>Output File:</strong> <a href='{result.output_file}'>{Path(result.output_file).name}</a></p>"
            
            if result.errors:
                html_content += f"<p><strong>Errors:</strong></p><pre>{''.join(result.errors)}</pre>"
            
            html_content += "</div>"
        
        html_content += """
    <div class="summary">
        <h2>Recommendations</h2>
        <ul>
            <li>Focus on reducing cyclomatic complexity in functions > 15</li>
            <li>Address high and medium security vulnerabilities first</li>
            <li>Eliminate code duplication through synthesis and refactoring</li>
            <li>Review files with high change frequency for stability issues</li>
            <li>Modernize code to C23 standards for improved safety</li>
        </ul>
    </div>
</body>
</html>
"""
        
        with open(report_file, 'w') as f:
            f.write(html_content)
        
        return str(report_file)

def main():
    parser = argparse.ArgumentParser(description='Run comprehensive code analysis on MINIX4')
    parser.add_argument('--workspace', default='.', help='Workspace root directory')
    parser.add_argument('--output', default='docs/analysis/advanced_analysis', 
                       help='Output directory for analysis results')
    parser.add_argument('--workers', type=int, default=4, 
                       help='Number of parallel workers')
    
    args = parser.parse_args()
    
    suite = CodeAnalysisSuite(args.workspace, args.output)
    results = suite.run_all_analyses(max_workers=args.workers)
    
    # Generate comprehensive report
    report_file = suite.generate_comprehensive_report()
    
    print(f"\n{'='*60}")
    print("ANALYSIS COMPLETE")
    print(f"{'='*60}")
    print(f"Comprehensive report: {report_file}")
    print(f"Output directory: {args.output}")
    
    # Summary statistics
    successful = len([r for r in results.values() if r.status == 'success'])
    total = len(results)
    print(f"Analyses completed: {successful}/{total}")
    
    if successful < total:
        print("\nFailed analyses:")
        for name, result in results.items():
            if result.status != 'success':
                print(f"  - {name}: {result.status}")

if __name__ == "__main__":
    main()
